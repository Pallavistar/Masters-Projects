require "test_helper"

class AdminTest< ActiveSupport::TestCase
  test "admin save" do
    #admin can be saved and destroyed
    tempadmin = users(:Pallavi)
    assert tempadmin.save
    assert tempadmin.destroy

    #admin can not be saved without sufficient data
    @admin2 = users(:Erik)
    assert_not_kind_of(Admin, @admin2, "Missing data for admin")
  end
end