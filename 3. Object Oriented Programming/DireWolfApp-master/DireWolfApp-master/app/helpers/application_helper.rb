module ApplicationHelper

  #A function to return a title used in the application view
  def title
    base_title = "DireWolf"
    if @title == nil
      base_title
    else
      "#{base_title} - #{@title}"
    end
  end
end