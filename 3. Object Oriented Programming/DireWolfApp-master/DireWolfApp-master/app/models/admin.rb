class Admin < User

  VALID_EMAIL_REGEX = /\A[\w+\-.]+@[a-z\d\-.]+\.[a-z]+\z/i

  validates :name,      :presence => true,
            :length => { :maximum => 50 }
  validates :username,  :presence => true,
            :length => { :maximum => 15 },
            :uniqueness => { :case_sensitive => false}
  validates :password,  :presence => true,
            confirmation: true,
            :length => { :within => (5..20) },
            :if => :password
  validates :email, presence: true, length: { maximum: 60}, format: { with: VALID_EMAIL_REGEX }

  before_save :encrypt_password, :if => :password

end