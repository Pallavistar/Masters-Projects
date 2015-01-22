require 'test_helper'

class JobTest < ActiveSupport::TestCase
  test "test tag" do
    @tag1 = Tag.new
    @tag1.job_id = jobs(:jobone).__id__
    @tag2 = Tag.new
    @tag2.job_id = jobs(:jobone).__id__
    #one job can have more than one tags
    assert_equal @tag1.job_id , @tag2.job_id
  end
end
