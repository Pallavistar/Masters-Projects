require 'test_helper'

class SessionsControllerTest < ActionController::TestCase
  def test_create
    post(:create, User: {username: "John" , Description: "Computer Graphics", employer_id: 1 , category_id: 1})
    assert_response :found
    assert_nil ::User.find_by_username("John")
  end
end
