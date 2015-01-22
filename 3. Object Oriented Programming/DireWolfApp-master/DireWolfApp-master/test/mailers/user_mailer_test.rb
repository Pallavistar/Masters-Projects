require 'test_helper'

class UserMailerTest < ActionMailer::TestCase
  test "new_job_application" do
    @user1 = users(:Pallavi)
    @job1 = jobs(:jobone)
    mail = UserMailer.new_job_application(@user1,@job1)
    assert_equal "New job application received", mail.subject
    assert_equal ["pallavi1206@yahoo.com"], mail.to
    #assert_equal ["DireWolf Jobs"], mail.from
    assert_match "Hi", mail.body.encoded
  end

end
