class JobsController < ApplicationController
  before_action :set_job, only: [:show, :edit, :update, :destroy]
  before_action :require_login
  before_action :check_if_employer
  before_action :job_check_for_id_job, only: [:show, :edit, :update, :destroy, :view_applications]
  before_action :job_check_for_id_application, only: [:change_status, :update_status]

  # GET /jobs
  # GET /jobs.json
  def index
    @jobs = Job.where(employer_id: session[:user_id])
    @category = Category.all
    @tags = Tag.all
    @employer = Employer.find(session[:user_id]).name
    @this_user = Employer.find(session[:user_id])
    @applications = Jobapplication.all
  end

  # GET /jobs/1
  # GET /jobs/1.json
  def show
    @category = Category.find(@job.category_id).name
    @employer = Employer.find(@job.employer_id).name
    tags_list = Tag.where(job_id: @job.id)
    @tags = ""
    tags_list.each do |tag|
      @tags += " " + tag.name.strip
    end
    @applications = Jobapplication.where(job_id: @job.id).count
  end

  # GET /jobs/new
  def new
    @job = Job.new
    @category = Category.all
    @employer_id = session[:user_id]
    @employer_name = Employer.find(@employer_id).name
  end

  # GET /jobs/1/edit
  def edit
    @category = Category.all
    @employer_id = session[:user_id]
    @employer_name = Employer.find(@employer_id).name
    tags_list = Tag.where(job_id: @job.id)
    tag_names = []
    tags_list.each do |tag|
      tag_names = tag_names << tag.name.strip
    end
    @tags = tag_names.join(",")
  end

  # POST /jobs
  # POST /jobs.json
  def create
    @job = Job.new(job_params)
    respond_to do |format|
      if @job.save
        tag_array = @job.get_tags
        tag_array.each do |a|
          tag = Tag.new
          tag.name = a.strip
          tag.job_id = @job.id
          tag.save
        end
        format.html { redirect_to @job, notice: 'Job was successfully created.' }
        format.json { render :show, status: :created, location: @job }
      else
        format.html { render :new }
        format.json { render json: @job.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /jobs/1
  # PATCH/PUT /jobs/1.json
  def update
    respond_to do |format|
      if @job.update(job_params)
        tags_list = Tag.where(job_id: @job.id)
        tags_list.each do |tag|
          tag.destroy
        end

        tag_array = @job.get_tags
        tag_array.each do |a|
          tag = Tag.new
          tag.name = a.strip
          tag.job_id = @job.id
          tag.save
        end
        format.html { redirect_to @job, notice: 'Job was successfully updated.' }
        format.json { render :show, status: :ok, location: @job }
      else
        format.html { render :edit }
        format.json { render json: @job.errors, status: :unprocessable_entity }
      end
    end
  end

  def update_status
    @application = Jobapplication.find(params[:id])
    @job = Job.find(@application.job_id)
    @employer = Employer.find(@job.employer_id)
    @jobseeker = Jobseeker.find(@application.jobseeker_id)
    respond_to do |format|
      if @application.update(update_status_params)

        UserMailer.application_status_update(@job,@employer,@jobseeker).deliver

        format.html { redirect_to view_applications_employer_path(@job), notice: 'Job was successfully updated.' }
        format.json { render :show, status: :ok, location: @job }
      else
        format.html { render :change_status }
        format.json { render json: @job.errors, status: :unprocessable_entity }
      end
    end
  end

  def view_applications
    @job = Job.find(params[:id])
    @applications = Jobapplication.where(job_id: @job.id)
    @jobseekers = Jobseeker.all
  end

  def change_status
    @application = Jobapplication.find(params[:id])
    @job_status = @application.status
  end

  # DELETE /jobs/1
  # DELETE /jobs/1.json
  def destroy
    @job.destroy
    respond_to do |format|
      format.html { redirect_to jobs_url, notice: 'Job was successfully deleted.' }
      format.json { head :no_content }
    end
  end

  def view_employer_profile
    @title = 'My Profile'
    @employer = Employer.find(session[:user_id])
  end

  def edit_employer_profile
    @title = 'Edit Profile'
    @employer = Employer.find(session[:user_id])
  end

  def update_employer_profile
    @user = Employer.find(session[:user_id])
    respond_to do |format|
      if @user.update(employer_update_params)
        format.html { redirect_to employer_profile_path, notice: 'User was successfully updated.' }
        format.json { render :show, status: :ok, location: @user }
      else
        format.html { render :edit_employer_profile }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  def new_employer_pass
    @title = 'Change Password'
    @user = User.find(session[:user_id])
  end

  def update_employer_password
    @user = Employer.find(session[:user_id])
    respond_to do |format|
      if @user.update(employer_password_params)
        format.html { redirect_to employer_profile_path, notice: 'Password was successfully updated.' }
        format.json { render :show, status: :ok, location: @user }
      else
        format.html { render :new_employer_pass }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_job
      @job = Job.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def job_params
      params.require(:job).permit(:title, :description, :employer_id, :category_id, :deadline, :tags)
    end

    def employer_update_params
      params.require(:employer).permit(:name, :email, :username, :phone, :type, :contact_name)
    end

    def update_status_params
      params.require(:jobapplication).permit(:status)
    end

    def employer_password_params
      params.require(:employer).permit(:password_hash, :password_salt, :password, :password_confirmation, :type)
    end
end
