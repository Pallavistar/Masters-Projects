require 'test_helper'

class JobsControllerTest < ActionController::TestCase
  def test_create
    post(:create, Job: {username: "Pal12" , password: 'robin'})
    assert_response :found
    assert_nil ::Job.find_by_employer_id("Google")
  end
end
