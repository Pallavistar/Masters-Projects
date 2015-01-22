class UserMailer < ActionMailer::Base
  default from: "DireWolf Jobs"

  # Subject can be set in your I18n file at config/locales/en.yml
  # with the following lookup:
  #
  #   en.user_mailer.new_job_application.subject
  #
  def new_job_application(user, job)
    @user_name = user.name.titleize
    @job_title = job.title

    mail to: user.email, subject: "New job application received"
  end

  def application_status_update(job, employer, jobseeker)
    @user_name = jobseeker.name.titleize
    @job_title = job.title
    @employer = employer.name.titleize

    mail to: jobseeker.email, subject: "Your job application status has been updated"
  end

end
