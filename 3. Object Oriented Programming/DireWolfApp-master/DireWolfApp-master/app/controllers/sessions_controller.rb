class SessionsController < ApplicationController
  def new
  end

  def create
    @user = User.authenticate(params[:username], params[:password])
    if @user
      session[:user_id] = @user.id
      session[:user_type] = @user.type
      flash[:notice] = "You've been logged in."
      if(@user.type == 'Admin')
        redirect_to users_path
      elsif(@user.type == 'Employer')
        redirect_to employer_home_path
      elsif(@user.type == 'Jobseeker')
        redirect_to jobseeker_home_path
      else
        flash[:alert] = "Could not log you in - user type not defined."
        redirect_to pages_home_path
      end
    else
      flash[:alert] = "There was a problem logging you in."
      redirect_to pages_home_path
    end
  end

  def destroy
    session[:user_id] = nil
    flash[:notice] = "You've been logged out successfully."
    redirect_to pages_home_path
  end

end
