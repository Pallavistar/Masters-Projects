class CreateJobs < ActiveRecord::Migration
  def change
    create_table :jobs do |t|
      t.string :title
      t.text :description
      t.integer :employer_id
      t.integer :category_id
      t.date :deadline

      t.timestamps
    end
  end
end
