require 'test_helper'

class JobTest < ActiveSupport::TestCase
  test "Jobseeker type" do
    #jobseeker should not have type employer
    @temp = users(:Google)
    assert_not_equal(@temp.type, 'Jobseeker')
  end
end
