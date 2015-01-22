require 'test_helper'

class JobseekersControllerTest < ActionController::TestCase
  def test_create
    post(:create, jobseeker: {name:  "Denerys", email: "denT123@gmail.com",
                              username: "denT123", password_hash: "temp den hash",
                              password_salt: "temp den salt", password: "denT@121",
                              password_confirmation: "denT@121", phone: "9191234566",
                              skills: "Java", resume: "temp data"})
    assert_response :found
    assert ::Jobseeker.find_by_name("Denerys")
  end
end
