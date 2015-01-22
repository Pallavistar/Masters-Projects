class PagesController < ApplicationController
  def home

  end

  def search
    @search_string = params[:q]
    @jobs = Job.search(params[:q],params[:titles],params[:desc],params[:emp],params[:cat],params[:tags])
    @category = Category.all
    @tags = Tag.all
    @employer = Employer.all

    @titles_true = params[:titles]
    @desc_true = params[:desc]
    @emp_true = params[:emp]
    @cat_true = params[:cat]
    @tags_true = params[:tags]
  end

  def preview
    @jobs = Job.all
    @category = Category.all
    @tags = Tag.all
    @employer = Employer.all
    @titles_true = @desc_true = @emp_true = @cat_true = @tags_true = true
  end
end
