require 'test_helper'

class ApplicationControllerTest < ActionController::TestCase
  setup do
    @controller = UsersController.new
  end

  def test_createusr
    post(:create, User: {username: "Yogest" , username: "ynandak" , password: 'ynandak'})
    assert_response :found
    assert_nil ::User.find_by_username("Ankita")
  end
end



