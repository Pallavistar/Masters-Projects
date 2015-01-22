json.array!(@jobs) do |job|
  json.extract! job, :id, :title, :description, :employer_id, :category_id, :deadline
  json.url job_url(job, format: :json)
end
