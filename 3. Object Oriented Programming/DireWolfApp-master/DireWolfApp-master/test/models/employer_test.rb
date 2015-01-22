require "minitest/autorun"
require "test_helper"
require "minitest/assertions"

class EmployerTest< ActiveSupport::TestCase
  test "employer type" do
    #type of user is Employer
    @emp1 = users(:Google)
    assert_kind_of(Employer, @emp1)
  end
end