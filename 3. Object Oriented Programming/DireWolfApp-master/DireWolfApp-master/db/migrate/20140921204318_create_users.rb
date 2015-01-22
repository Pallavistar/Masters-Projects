class CreateUsers < ActiveRecord::Migration
  def change
    create_table :users do |t|
      t.string :name
      t.string :email
      t.string :username
      t.string :password_hash
      t.string :password_salt
      t.string :phone
      t.string :contact_name
      t.string :skills
      t.string :resume
      t.string :type

      t.timestamps
    end
  end
end
