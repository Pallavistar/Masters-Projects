require 'test_helper'

class CategoryTest < ActiveSupport::TestCase
   test "Category exists" do
     #Each job should have a category.
     @temp1 = jobs(:jobone)
     assert_not_nil @temp1.category_id

     # more than one job can have same category
     @temp2 = jobs(:jobtwo)
     assert_equal(@temp1.category_id, @temp2.category_id)
   end
end
