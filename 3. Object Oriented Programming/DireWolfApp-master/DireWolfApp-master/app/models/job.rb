class Job < ActiveRecord::Base
  attr_accessor :tags
  belongs_to :employer
  belongs_to :category
  has_many :jobapplications, dependent: :destroy
  has_many :tags, dependent: :destroy

  #before_save :add_tags, :if => :tags

  def get_tags
    self.tags.split(',')
  end

  def self.search(q,titles,desc,emp,cat,tags)
    jobs = []
    if q
      #find(:all, :conditions => ['name LIKE ?', "%#{q}%"])
      if titles
        jobs.concat Job.where('title like ?',"%#{q}%")
      end
      if desc
        jobs.concat Job.where('description like ?',"%#{q}%")
      end
      if emp
        jobs.concat Job.where(employer_id: Employer.where('name like ?',"%#{q}%"))
      end
      if cat
        jobs.concat Job.where(category_id: Category.where('name like ?',"%#{q}%"))
      end
      if tags
        jobs.concat Job.where(id: Tag.where('name like ?',"%#{q}%").map{|x| x.job_id})
      end
      jobs.uniq!
      jobs
    else
      jobs.concat Job.all
      jobs
    end
  end

  def self.recommendations(user)
    apps = Jobapplication.where(jobseeker_id: user)
    job_ids = apps.map {|x| x.job_id}
    jobs_applied = Job.where(id: job_ids)

    tags = (Tag.where(job_id: jobs_applied).map{|x| x.name}).uniq
    tag_recos = Job.where(id: Tag.where(name: tags).map{|x| x.job_id}.uniq)

    categories = Category.where(id: jobs_applied.map{|x| x.category_id}.uniq)
    cat_recos = Job.where(category_id: categories)

    employers = Employer.where(id: jobs_applied.map{|x| x.employer_id}.uniq)
    emp_recos = Job.where(employer_id: employers)

    return tag_recos, cat_recos, emp_recos
  end


end