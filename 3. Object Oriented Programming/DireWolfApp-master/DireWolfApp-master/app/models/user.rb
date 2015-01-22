class User < ActiveRecord::Base
  attr_accessor :password, :password_confirmation

  def self.authenticate(username, password)
    user = User.where(username: username).first
    if user && user.password_hash == user.encrypt(password)
      user
    else
      nil
    end
  end

  def encrypt_password
    self.password_salt = make_salt unless self.password_salt
    self.password_hash = encrypt(password)
  end

  def encrypt(string)
    secure_hash("#{self.password_salt}_-_#{string}")
  end

  private

  def secure_hash(string)
    Digest::SHA2.hexdigest(string)
  end

  def make_salt
    require 'securerandom'
    SecureRandom.hex
  end
end
