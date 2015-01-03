/*
  ramdisk using FUSE
  Author	:	Pallavi Deo.

  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall ramdisk.c `pkg-config fuse --cflags --libs` -o ramdisk
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

size_t mainSize = 1024;		// default size of entire RAMFS
size_t curSize =0;			// size of entire filesystem at given time
char* ecpath;				// Path for extra credit

typedef struct diskds{
	int type; 		// 0=file 1=dir
	int id;
	char* name;
	size_t filesize;

	struct diskds* nextdir;	// directories in same parent folder
	struct diskds* nextfile;	// files in same parent folder
	struct diskds* subdir;	// linked list of its child direcories, this is first
	
	char* databuf;
}diskds;
diskds *mydisk, *root;

/*--------------------------- getdiskds()--------------------------- 
*	searches for given path from root directory
*	populates "mydisk" if found
*	otherwise exits
*-------------------------------------------------------------------*/
void getdiskds(const char *givenPath){
	//strcpy(root->name , givenPath);
	printf("getdiskds entered\n");

	int found =0;
	char *path = malloc (strlen(givenPath));
	strcpy (path , givenPath);
	
	char *subpath= malloc (strlen(givenPath));
	diskds *tempdisk, *curdir;

	curdir = root; tempdisk = root;
	printf("current dir is %u \n", curdir);
	subpath = strtok(path, "/");

	while(subpath!= NULL){
		printf("searching %s in %s\n", subpath, curdir->name);
		if (curdir->subdir !=NULL){
			printf("subdir not null %u\n", curdir->subdir);
			tempdisk = curdir->subdir;
			if (!strcmp(tempdisk->name, subpath)){
				found =1;
				printf("found in subdir, %u", tempdisk);
			}
		
			while(!found && tempdisk->nextdir != NULL){
				if (!strcmp(tempdisk->nextdir->name, subpath)) {//found
					tempdisk = tempdisk->nextdir;
					found =1;
					printf("found in subdir, %u", tempdisk);
					break;
				}
				tempdisk = tempdisk->nextdir;
				printf("One iteration done\n");
			}
		}
		// if directory not found, search in files
		if (!found) tempdisk = curdir;
		while(!found && tempdisk->nextfile != NULL){
			if (!strcmp(tempdisk->nextfile->name, subpath)) {//found
				tempdisk = tempdisk->nextfile;
				found =1;
				printf("found in subfile, %u", tempdisk);
				break;
			}
			 tempdisk = tempdisk->nextfile;
		}

		// if found, tempdisk points to it. so, check for the next element in path
		if (found){
			subpath = strtok(NULL, "/");		//Now search for this dir inside first
			curdir = tempdisk;
			found =0;
		}
		else{	// bad file path,or it might be mkdir call
			printf("Path not found\n");
			mydisk->type=2;
			return -ENOENT;
		}
	}	// no further tokens
		mydisk = curdir;
		//mydisk->type		= curdir ->type;	printf("%d\n", mydisk->type);
		//mydisk->id			= curdir ->id;		printf("%d\n", mydisk->id);
		//printf("root name is %s %s\n", mydisk->name, curdir ->name);
		//strcpy(mydisk->name, curdir ->name);	printf("%s\n", mydisk->name);

		return 0;
}

/*-------------------------------------------------------------------*/

static int ramdisk_getattr(const char *path, struct stat *stbuf)
{
	strcpy(root->name , path);
	memset(stbuf, 0, sizeof(struct stat));
	printf("getattr entered\n");
	printf("Path is %s\n", path);
	int res = 0;

	if (strcmp(path, "/") == 0) {		// this is the root
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		stbuf->st_size=4096;
		stbuf->st_uid=getuid();
		stbuf->st_gid=getgid();
		return 0;
	} else {							// search in directory stuct of root
		printf("Calling getdiskds from getattr\n");
		mydisk = root;
		getdiskds(path);	// populate diskds data
		printf("Back from getdiskds into getattr\n");
		if (mydisk->type==1){		// if directory with that name is present
			printf("file type is 1\n");
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
			stbuf->st_uid=getuid();
			stbuf->st_gid=1000;
			stbuf->st_size = 1024;		// may need to change this
		}else if (mydisk->type==0){	// if file with that name is present	
			printf("file type is 0\n");
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_uid=getuid();
			stbuf->st_gid=getgid();
			stbuf->st_size = 1024;		// may need to change this
		} else{
			mydisk->type=1;
			res = -ENOENT;
		}
	}
	return res;
}

/*-------------------------------------------------------------------*/

static int ramdisk_readdir(const char *givenPath, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	strcpy(root->name , givenPath);
	printf("readdir entered\n");
	mydisk = root;
	char* path = malloc (strlen(givenPath));
	strcpy(path ,givenPath);
	getdiskds(path);	// This is the current dir we are in

	(void) offset;
	(void) fi;
	printf("%s\n", root->name);
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	printf ("back from getdatads\n");
	// code to populate buf with ramdisk file structure
	// Fill in all the directories first
	diskds *temp = mydisk->subdir;
	while (temp != NULL){
		printf("%s\n",temp->name);
		filler(buf, temp->name, NULL, 0);
		temp=temp->nextdir;
	}
	temp = mydisk->nextfile;
	// then the files
	while (temp != NULL){
		printf("%s\n",temp->name);
		filler(buf, temp->name, NULL, 0);
		temp=temp->nextfile;
	}
	return 0;
}

/*-------------------------------------------------------------------*/

static int ramdisk_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

/*-------------------------------------------------------------------*/

static int ramdisk_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	strcpy(root->name , path);
	printf("ramdisk read entered\n");
	size_t len;
	(void) fi;
	mydisk = root;
	getdiskds(path);	// populate diskds data in mydisk variable

	if(size > (mydisk->filesize - offset)){
		memcpy(buf,mydisk->databuf+offset,mydisk->filesize - offset);
		return mydisk->filesize - offset;
	}else{
		memcpy(buf,mydisk->databuf+offset,size);
		return size;
	}
	// check if the read starts outside the file & if there is a need to return zero.
}

/*-------------------------------------------------------------------*/

int ramdisk_create (const char * newpath, mode_t mode, struct fuse_file_info * fileinfo) {
	/*	Get current disk info, which will be parent file
	*	newpath = <some path> +  <new file name>
	*	thus, first parse the given path to get to the parent directory & new file name
	*/
	printf("create entered\n");
	char* temp = malloc (10+strlen(newpath));
	char* oldpath = malloc (10+strlen(newpath));
	char* newname = malloc (10+strlen(newpath));
	char* path  = malloc (10+strlen(newpath));
	
	strcpy(path, newpath);
	temp = strtok(path, "/");

	while (temp!= NULL){
		strcpy (newname, temp);
		temp = strtok(NULL,"/");
	}
	strcpy(path, newpath);
	oldpath = strtok(path, newname);

	getdiskds(oldpath);
	// IMPORTANT : PUT A CHECK FOR TOTAL FILE SIZE

	// form a new file, initialize
	diskds *newdisk	= malloc (sizeof(diskds));	
	newdisk->name = malloc (10);
	strcpy(newdisk->name,newname);
	newdisk->type 		= 0;
	newdisk->filesize 	= 0;
	newdisk->nextdir 	= NULL;
	newdisk->subdir 	= NULL;
	newdisk->nextfile	= NULL;

	diskds *tempdisk;
	tempdisk = mydisk;
	printf("mydisk %u \n tempdisk %u\n", mydisk, tempdisk);
	printf("mydisk %s \n", mydisk->name);
	//find the last file in linked list
	while (tempdisk->nextfile != NULL){
		tempdisk = tempdisk->nextfile;
	}
	tempdisk->nextfile = newdisk; 
	return 0;
}

/*-------------------------------------------------------------------*/

int ramdisk_write (const char *givenPath, const char *buf, size_t writeSize, off_t offset, struct fuse_file_info * info) {
	strcpy(root->name , "/");
	printf("ramdisk write entered\n");
	// check size
	if(curSize + writeSize > mainSize)
		return -ENOSPC;
	curSize+=writeSize;
	char* path = malloc (strlen(givenPath));
	strcpy(path, givenPath);
	getdiskds(path);

	//set size
	if(mydisk->databuf == NULL){
		mydisk->databuf	=(char *)malloc(writeSize);
		mydisk->filesize	= writeSize;
	}else{
		if(offset > 0)
			mydisk->filesize	+= mydisk->filesize+writeSize; // i think offset should be subtracted here, not sure
		else
			mydisk->filesize	= writeSize;
		mydisk->databuf		= realloc(mydisk->databuf,mydisk->filesize);
	}
	//actually write
	memcpy((mydisk->databuf)+offset,buf,writeSize);
	return writeSize;
}

/*-------------------------------------------------------------------*/

int ramdisk_mkdir (const char * newname, mode_t mode) {
	if(curSize + sizeof(diskds) > mainSize)
		return -ENOSPC;
	curSize+=sizeof(diskds);

	printf("ramdisk mkdir entered\n");
	//IMPORTANT put a check for  size

	diskds *newdisk;
	newdisk = malloc (sizeof(diskds));
	printf("New disk created\n");

	newdisk->type 		= 1;
	printf("type assigned, new name is %s\n", newname);
	
	char* tempname = malloc (strlen(newname));
	strcpy (tempname,newname);
	char* name= malloc (strlen(newname));

	tempname = strtok(newname,"/");
	while (tempname!= NULL){
		strcpy(name , tempname);
		tempname = strtok(NULL,"/");
	}	
	printf ("truncated name is %s\n", name);
	newdisk->name= malloc (20);
	strcpy(newdisk->name, name);
	printf("%s\n",newdisk->name);
	newdisk->filesize 	= 0;
	newdisk->nextdir 	= NULL;
	newdisk->subdir 	= NULL;
	newdisk->nextfile	= NULL;

	//give referance to last nextdir of parent
	diskds *tempdisk;	
	mydisk = root;
	//printf("before strtok new name: %s tempname: %s\n", newname, tempname);
	char* newname1= malloc (strlen(newname));
	strcpy (newname1, newname);
	tempname = strtok (newname1, name);
	//printf("before calling getdiskds new name: %s tempname: %s\n", newname, tempname);
	getdiskds(tempname); // found the parent
	//printf("Done searching for parent %s\n", mydisk->name);
	if (mydisk->subdir == NULL){
		/*printf("Assigning to subdir\n");
		mydisk->subdir = malloc (sizeof(diskds));
		mydisk->subdir->name = malloc (20);
		mydisk->subdir->type = newdisk->type;
		mydisk->subdir->id = newdisk->id;
		mydisk->subdir->name = newdisk->name;
		mydisk->subdir->filesize= newdisk->filesize;
		printf("%s\n",mydisk->subdir->name );*/
		mydisk->subdir = newdisk;
	}
	else {
			tempdisk = mydisk->subdir;
		while (tempdisk->nextdir != NULL){
			//find the last file in linked list
			tempdisk = tempdisk->nextdir;
		}
		/*printf("Assigning pointer to child's next\n");
		tempdisk->nextdir = malloc (sizeof(diskds));
		tempdisk->nextdir->name = malloc (sizeof (char)*10);
		tempdisk->nextdir->type = newdisk->type;
		tempdisk->nextdir->id = newdisk->id;
		tempdisk->nextdir->name = newdisk->name;
		tempdisk->nextdir->filesize= newdisk->filesize;*/
		mydisk->nextdir = newdisk;
	}
	printf("exiting mkdir\n");
	return 0;

}

/*-------------------------------------------------------------------*/

int ramdisk_rmdir (const char * dirname) {
	
	printf("ramdisk rmdir entered\n");

	diskds  *temp;
	temp  = root;
	int found =0;
	char * dir = malloc (strlen(dirname));
	strcpy(dir,dirname);

	if(temp->subdir != NULL){
		printf("checking subdir %s %s\n", temp->name, temp->subdir->name);
		if (!strcmp (temp->subdir->name, dir))
			found =1;
		else{
			printf("subdir doesnot match\n");
			temp = temp->subdir;
			while (temp->nextdir != NULL && found ==0){
				if (!strcmp (temp->nextdir->name, dir))
					found =1;
				else temp = temp->nextdir;
			}
		}
	}
	if (found ==0 )
		return -ENOENT;
	printf("node to remove is found\n");
	// IMPORTANT do the size adjustments

	if (!strcmp (temp->nextdir->name, dir)) {
		if (temp->nextdir->nextdir !=NULL)
			temp->nextdir = temp->nextdir->nextdir;
		else temp->nextdir = NULL;
	}
	else if (!strcmp (temp->subdir->name, dir)){
		if (temp->subdir->nextdir != NULL)
			temp->subdir = temp->subdir->nextdir;
		else
			temp->subdir = NULL;
	}
	printf("Deleted, exiting rmdir\n");
	return 0;
}

/*-------------------------------------------------------------------*/

static struct fuse_operations ramdisk_oper = {
	.getattr	= ramdisk_getattr,
	.readdir	= ramdisk_readdir,
	.open		= ramdisk_open,
	.read		= ramdisk_read,
	.create		= ramdisk_create,
	.write		= ramdisk_write,
	.mkdir		= ramdisk_mkdir,
	.rmdir		= ramdisk_rmdir,
};

/*-------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	if (argc >4) {
		printf ("Too many arguments\n");
		printf("Usage:  ramdisk <mount_point> <size> [<filename>]\n");
		exit(0);
	}
	else if (argc==4){ 
		ecpath = argv[3];
		mainSize = atoi (argv[2]); 
		if (mainSize <=0){
			printf("Size should be greater than zero\n");
			exit(0);
		}
	}
	else if (argc ==3){
		printf ("No path specified! Starting up a fresh (empty) filesystem.\n");
		mainSize = atoi (argv[2]);
		if (mainSize <=0){
			printf("Size should be greater than zero\n");
			exit(0);
		}		
	}
	else if (argc ==2){
		printf("Usage:  ramdisk <mount_point> <size> [<filename>]\n");
		exit(0);
	}
	if (argc>2){
		argv[3] = NULL; argv[2] = NULL; argc=2;		// We don't want to pass these details to FUSE
	}
	root = malloc (sizeof(diskds));	
	root->name = malloc (50);

	root->type	= 1;
	root->id	= 0;
	strcpy (root->name,"/");
	
	//argc =3;
	//argv[2] = "-d";

	return fuse_main(argc, argv, &ramdisk_oper, NULL);
}
