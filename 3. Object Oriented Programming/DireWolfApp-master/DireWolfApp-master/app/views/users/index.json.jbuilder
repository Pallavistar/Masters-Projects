json.array!(@users) do |user|
  json.extract! user, :id, :name, :email, :username, :password_hash, :password_salt, :phone, :contact_name, :skills, :resume, :type
  json.url user_url(user, format: :json)
end
