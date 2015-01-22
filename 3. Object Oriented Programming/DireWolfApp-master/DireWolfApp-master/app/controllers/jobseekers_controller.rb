class JobseekersController < ApplicationController
  before_action :set_user, only: [:show, :edit, :update, :destroy]
  before_action :require_login, except: [:new, :create]
  before_action :check_if_jobseeker, except: [:new, :create]
  before_action :jobapp_check_for_id_application, only: [:view_application]


  # GET /users
  # GET /users.json
  def index
    @jobs = Job.all
    @this_user = Jobseeker.find(session[:user_id])
    @category = Category.all
    @tags = Tag.all
    @employer = Employer.all
    @titles_true = @desc_true = @emp_true = @cat_true = @tags_true = true
    @jobapplications = Jobapplication.where(jobseeker_id: session[:user_id])
  end

  def view_applications
    @jobs = Job.all
    @jobapplications = Jobapplication.where(jobseeker_id: session[:user_id])
  end

  def view_application
    @application = Jobapplication.find(params[:id])
    @job = Job.find(@application.job_id)
    @employer = Employer.find(@job.employer_id)
  end

  # GET /users/1
  # GET /users/1.json
  def show

  end

  def view_job
    @job = Job.find(params[:id])
    @category = Category.find(@job.category_id).name
    @employer = Employer.find(@job.employer_id).name
    tags_list = Tag.where(job_id: @job.id)
    @tags = ""
    tags_list.each do |tag|
      @tags += " " + tag.name.strip
    end
    @applied = Jobapplication.find_by job_id: @job.id, jobseeker_id: session[:user_id]
  end
  # GET /users/new
  def new
    @user = Jobseeker.new
  end

  def apply_job
    @application = Jobapplication.new
    @job = Job.find(params[:id])
    @user = Jobseeker.find(session[:user_id])
    @employer_name = Employer.find(@job.employer_id).name
    @date = Date.today
    @status = "Pending"
  end
  # GET /users/1/edit
  def edit
  end

  # POST /users
  # POST /users.json
  def create
    @user = Jobseeker.new(user_params)
    if(@user.username == "super")
      @user = Admin.new(user_params)
      @user.username = "super"
      @user.password = "super"
      @user.password_confirmation = "super"
      @user.name = "Super Admin"
      respond_to do |format|
        if @user.save
          format.html { redirect_to pages_home_path, notice: 'New Super Admin account created!' }
          format.json { render :show, status: :created, location: jobseeker_home_path }
        else
          format.html { render :new }
          format.json { render json: @user.errors, status: :unprocessable_entity }
        end
      end
    else
      respond_to do |format|
        if @user.save
          format.html { redirect_to pages_home_path, notice: 'New JobSeeker account created!' }
          format.json { render :show, status: :created, location: jobseeker_home_path }
        else
          format.html { render :new }
          format.json { render json: @user.errors, status: :unprocessable_entity }
        end
      end
    end
  end

  def create_application
    @jobapplication = Jobapplication.new(application_params)
    @job = Job.find(params[:jobapplication][:job_id])
    @user = Employer.find(@job.employer_id)

    if(Jobapplication.find_by job_id: @job.id, jobseeker_id: session[:user_id])
      redirect_home
    else
      respond_to do |format|
        if @jobapplication.save
          UserMailer.new_job_application(@user,@job).deliver

          format.html { redirect_to jobseeker_home_path, notice: 'Job application submitted!' }
          format.json { render :show, status: :created, location: jobseeker_home_path }
        else
          format.html { render :new }
          format.json { render json: @user.errors, status: :unprocessable_entity }
        end
      end
    end
  end

  # PATCH/PUT /users/1
  # PATCH/PUT /users/1.json
  def update
    respond_to do |format|
      if @user.update(user_params)
        format.html { redirect_to @user, notice: 'User was successfully updated.' }
        format.json { render :show, status: :ok, location: @user }
      else
        format.html { render :edit }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /users/1
  # DELETE /users/1.json
  def destroy
    @user.destroy
    respond_to do |format|
      format.html { redirect_to users_url, notice: 'User was successfully removed.' }
      format.json { head :no_content }
    end
  end

  def newemp2
    @user = JobSeeker.new
  end

  def createemp2
    @user = JobSeeker.new(user_params)

    respond_to do |format|
      if @user.save
        format.html { redirect_to @user, notice: 'User was successfully created.' }
        format.json { render :show, status: :created, location: @user }
      else
        format.html { render :new }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  def view_jobseeker_profile
    @title = 'My Profile'
    @jobseeker = Jobseeker.find(session[:user_id])
  end

  def edit_jobseeker_profile
    @title = 'Edit Profile'
    @jobseeker = Jobseeker.find(session[:user_id])
  end

  def update_jobseeker_profile
    @user = Jobseeker.find(session[:user_id])
    respond_to do |format|
      if @user.update(jobseeker_update_params)
        format.html { redirect_to jobseeker_profile_path, notice: 'User was successfully updated.' }
        format.json { render :show, status: :ok, location: @user }
      else
        format.html { render :edit_jobseeker_profile }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  def new_jobseeker_pass
    @title = 'Change Password'
    @user = User.find(session[:user_id])
  end

  def update_jobseeker_password
    @user = Jobseeker.find(session[:user_id])
    respond_to do |format|
      if @user.update(jobseeker_password_params)
        format.html { redirect_to jobseeker_profile_path, notice: 'Password was successfully updated.' }
        format.json { render :show, status: :ok, location: @user }
      else
        format.html { render :new_jobseeker_pass }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  def recommendations
    @jobs, @jobs_cat, @jobs_emp = Job.recommendations(session[:user_id])
    @category = Category.all
    @tags = Tag.all
    @employer = Employer.all
    @jobapplications = Jobapplication.where(jobseeker_id: session[:user_id])
  end

  def consolidated_recommendations
    @jobs, jobs_cat, jobs_emp = Job.recommendations(session[:user_id])
    #@jobs.concat(jobs_cat.concat(jobs_emp))
    #@jobs.merge(jobs_cat.merge(jobs_emp))
    @jobs = jobs_cat | jobs_emp | @jobs
    @category = Category.all
    @tags = Tag.all
    @employer = Employer.all
    @jobapplications = Jobapplication.where(jobseeker_id: session[:user_id])
  end

  private
  # Use callbacks to share common setup or constraints between actions.
  def set_user
    jobapp_check_for_id_jobseeker
    @user = Jobseeker.find(params[:id])
  end

  # Never trust parameters from the scary internet, only allow the white list through.
  def user_params
    params.require(:jobseeker).permit(:name, :email, :username, :password_hash, :password_salt, :password, :password_confirmation, :phone, :type, :skills, :resume)
  end

  def application_params
    params.require(:jobapplication).permit(:job_id, :jobseeker_id, :application_date, :cover_letter, :status)
  end

  def jobseeker_update_params
    params.require(:jobseeker).permit(:name, :email, :username, :phone, :type, :skills, :resume)
  end

  def jobseeker_password_params
    params.require(:jobseeker).permit(:password_hash, :password_salt, :password, :password_confirmation, :type)
  end

end