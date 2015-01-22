class CreateJobapplications < ActiveRecord::Migration
  def change
    create_table :jobapplications do |t|
      t.integer :job_id
      t.integer :jobseeker_id
      t.date :application_date
      t.text :cover_letter
      t.string :status

      t.timestamps
    end
  end
end
