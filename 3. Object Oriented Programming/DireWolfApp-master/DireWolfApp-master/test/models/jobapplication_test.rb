require 'test_helper'

class JobTest < ActiveSupport::TestCase
  test "Job application" do
    @app1 = Jobapplication.new
    @app1.job_id = 1
    @app1.jobseeker_id = users(:Pallavi).__id__

    assert @app1.save
  end
end
