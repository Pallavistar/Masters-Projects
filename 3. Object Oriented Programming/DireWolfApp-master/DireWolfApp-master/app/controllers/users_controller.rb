class UsersController < ApplicationController
  before_action :set_user, only: [:show, :edit, :update, :destroy]
  before_action :require_login
  before_action :check_if_admin

  # GET /users
  # GET /users.json
  def index
    @title = 'Home'
    @this_user = Admin.find(session[:user_id])
    @users = User.all
    @categories = Category.all
  end

  # GET /users/1
  # GET /users/1.json
  def show
    @title = 'View User'
  end

  # GET /users/new
  def new
    @title = 'New'
    @user = Admin.new
  end

  # GET /users/1/edit
  def edit
    @title = 'Edit User'
  end

  # POST /users
  # POST /users.json
  def create
    @user = Admin.new(user_params)

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

  # PATCH/PUT /users/1
  # PATCH/PUT /users/1.json
  def update
    if (@user.username == "super")
      redirect_home
    else
      respond_to do |format|
        if @user.update(user_params_update)
          format.html { redirect_to @user, notice: 'User was successfully updated.' }
          format.json { render :show, status: :ok, location: @user }
        else
          format.html { render :edit }
          format.json { render json: @user.errors, status: :unprocessable_entity }
        end
      end
    end
  end

  def update_password
    @user = User.find(params[:id])
    if (@user.username == "super")
      redirect_home
    else
      respond_to do |format|
        if @user.update(user_params_update_password)
          format.html { redirect_to @user, notice: 'User was successfully updated.' }
          format.json { render :show, status: :ok, location: @user }
        else
          format.html { render :new_pass }
          format.json { render json: @user.errors, status: :unprocessable_entity }
        end
      end
    end
  end

  # DELETE /users/1
  # DELETE /users/1.json
  def destroy
    if (@user.username == "super")
      redirect_home
    else
      @user.destroy
      respond_to do |format|
        format.html { redirect_to users_url, notice: 'User was successfully removed.' }
        format.json { head :no_content }
      end
    end
  end

  def destroy_jobseeker
    @user = Jobseeker.find(params[:id])
    @user.destroy
    respond_to do |format|
      format.html { redirect_to users_url, notice: 'User was successfully removed.' }
      format.json { head :no_content }
    end
  end

  def new_pass
    @title = 'Change Password'
    @user = User.find(params[:id])
  end

  def view_jobseeker
    @title = 'View Job Seeker'
    @job_seeker = Jobseeker.find(params[:id])
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_user
      @user = Admin.find(params[:id])
    end


    # Never trust parameters from the scary internet, only allow the white list through.
    def user_params
      params.require(:admin).permit(:name, :email, :username, :password_hash, :password, :password_confirmation, :password_salt, :phone, :type)
    end

  def user_params_update
    params.require(:admin).permit(:name, :email, :username, :phone, :type)
  end

  def user_params_update_password
    if(params[:admin])
      params.require(:admin).permit(:password, :password_confirmation)
    elsif(params[:employer])
      params.require(:employer).permit(:password, :password_confirmation)
    end
  end
end
