require "test_helper"

class UserTest < ActiveSupport::TestCase
  test "auth user" do
    @temp = User.new
    assert @temp.save
    assert @temp.destroy
  end
end
