class Jobapplication < ActiveRecord::Base
  belongs_to :jobseeker
  belongs_to :job
  #attr_accessor :status
end
