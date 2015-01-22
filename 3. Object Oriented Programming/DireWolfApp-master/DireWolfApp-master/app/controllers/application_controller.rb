class ApplicationController < ActionController::Base
  # Prevent CSRF attacks by raising an exception.
  # For APIs, you may want to use :null_session instead.
  protect_from_forgery with: :exception

  private

  def require_login
    unless session[:user_id]
      flash[:error] = "You must be logged in to access this page"
      redirect_to pages_home_path
    end
  end

  def check_if_admin
    unless session[:user_type] == 'Admin'
      redirect_home
    end
  end

  def check_if_employer
    unless session[:user_type] == 'Employer'
      redirect_home
    end
  end

  def check_if_jobseeker
    unless session[:user_type] == 'Jobseeker'
      redirect_home
    end
  end

  def job_check_for_id_job
    job = Job.find(params[:id])
    unless job.employer_id == session[:user_id]
      redirect_home
    end
  end

  def job_check_for_id_application
    jobapp = Jobapplication.find(params[:id])
    job = Job.find(jobapp.job_id)
    unless job.employer_id == session[:user_id]
      redirect_home
    end
  end

  def jobapp_check_for_id_application
    jobapp = Jobapplication.find(params[:id])
    unless jobapp.jobseeker_id == session[:user_id]
      redirect_home
    end
  end

  def jobapp_check_for_id_jobseeker
    job_seeker = Jobseeker.find(params[:id])
    unless job_seeker.id == session[:user_id]
      redirect_home
    end
  end

  def redirect_home
    flash[:error] = "You can't do that!"
    if(session[:user_type] == 'Admin' )
      redirect_to users_path
    elsif(session[:user_type] == 'Employer' )
      redirect_to employer_home_path
    elsif(session[:user_type] == 'Jobseeker' )
      redirect_to jobseeker_home_path
    else
      redirect_to '/'
    end
  end

end

