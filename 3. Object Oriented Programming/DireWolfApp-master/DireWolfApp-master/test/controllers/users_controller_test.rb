require 'test_helper'

class UsersControllerTest < ActionController::TestCase
  def test_create
    post(:create, User: {username: "Ankita" , username: "Ank25" , password: 'pise'})
    assert_response :found
    assert_nil ::User.find_by_username("Ankita")
  end
end
