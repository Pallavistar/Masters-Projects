require 'test_helper'

class JobTest < ActiveSupport::TestCase
   test "Job Exists" do
     #search can be performed by employer & category
     assert true , ::Job.find_by_employer_id('Google')
     assert true , ::Job.find_by_category_id('Networking')
   end
end
