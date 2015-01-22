require 'test_helper'

class EmployersControllerTest < ActionController::TestCase
  setup do
    @emp = users(:Google)
    @controller = EmployersController.new
  end
  test "should create employer" do
    post :create, user: { name: @emp.name, type: @emp.type }
    assert_response :found
  end
end
