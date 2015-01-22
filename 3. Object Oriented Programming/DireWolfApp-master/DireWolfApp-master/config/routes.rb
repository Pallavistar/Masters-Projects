Rails.application.routes.draw do

  resources :jobs

  resources :categories

  get '/login' => 'pages#home'
  post '/login' => 'sessions#create', as: :log_in
  get '/logout' => 'sessions#destroy', as: :log_out

  get 'users/newpass/:id', to: 'users#new_pass', as: :new_pass

  get '/employer/home', to: 'jobs#index', as: :employer_home
  get '/jobseeker/home', to: 'jobseekers#index', as: :jobseeker_home

  post '/jobseeker/home', to: 'jobseekers#create_application', as: :jobapplications
  get '/jobseeker/applications', to: 'jobseekers#view_applications', as: :view_applications
  get '/jobseeker/application/:id', to: 'jobseekers#view_application', as: :view_application

  get '/jobs/applications/:id', to: 'jobs#view_applications', as: :view_applications_employer

  get 'job/changestatus/:id', to: 'jobs#change_status', as: :change_status
  patch 'job/changestatus/:id', to: 'jobs#update_status', as: :jobapplication

  patch 'users/newpass/:id', to: 'users#update_password', as: :pass_update
  #post ''

  get 'pages/home'
  get 'preview', to: 'pages#preview', as: :preview_jobs

  get 'jobs/view/:id', to: 'jobseekers#view_job', as: :view_job
  get 'jobs/apply/:id', to: 'jobseekers#apply_job', as: :apply_job
  #get 'jobs/view/:id' => 'jobseekers#view_job', :as => 'view_job_path'

  get 'jobseekers/:id', to: 'users#view_jobseeker', as: :view_jobseeker
  delete 'jobseekers/:id', to: 'users#destroy_jobseeker', as: :destroy_jobseeker

  get 'employer/view', to: 'jobs#view_employer_profile', as: :employer_profile
  get 'employer/edit', to: 'jobs#edit_employer_profile', as: :edit_employer_profile
  get 'employer/newpass', to: 'jobs#new_employer_pass', as: :new_employer_pass

  match 'employer/password/update', to: 'jobs#update_employer_password', via: [:patch, :put], as: :update_employer_password
  match 'employer/update', to: 'jobs#update_employer_profile', via: [:patch, :put], as: :update_employer_profile

  get 'jobseeker/view', to: 'jobseekers#view_jobseeker_profile', as: :jobseeker_profile
  get 'jobseeker/edit', to: 'jobseekers#edit_jobseeker_profile', as: :edit_jobseeker_profile
  get 'jobseeker/newpass', to: 'jobseekers#new_jobseeker_pass', as: :new_jobseeker_pass

  match 'jobseeker/password/update', to: 'jobseekers#update_jobseeker_password', via: [:patch, :put], as: :update_jobseeker_password
  match 'jobseeker/update', to: 'jobseekers#update_jobseeker_profile', via: [:patch, :put], as: :update_jobseeker_profile

  get 'jobseeker/create', to: 'jobseekers#new', as: :create_new_jobseeker
  get 'jobseeker/recommendations', to: 'jobseekers#recommendations', as: :recommendations
  get 'jobseeker/recommendations/consolidated', to: 'jobseekers#consolidated_recommendations', as: :consolidated_recommendations

  get '/search', to: 'pages#search', as: :search

  resources :users
  resources :admins, controller: 'users'
  resources :employers
  resources :jobseekers

  root 'pages#home'
  # The priority is based upon order of creation: first created -> highest priority.
  # See how all your routes lay out with "rake routes".

  # You can have the root of your site routed with "root"
  # root 'welcome#index'

  # Example of regular route:
  #   get 'products/:id' => 'catalog#view'

  # Example of named route that can be invoked with purchase_url(id: product.id)
  #   get 'products/:id/purchase' => 'catalog#purchase', as: :purchase

  # Example resource route (maps HTTP verbs to controller actions automatically):
  #   resources :products

  # Example resource route with options:
  #   resources :products do
  #     member do
  #       get 'short'
  #       post 'toggle'
  #     end
  #
  #     collection do
  #       get 'sold'
  #     end
  #   end

  # Example resource route with sub-resources:
  #   resources :products do
  #     resources :comments, :sales
  #     resource :seller
  #   end

  # Example resource route with more complex sub-resources:
  #   resources :products do
  #     resources :comments
  #     resources :sales do
  #       get 'recent', on: :collection
  #     end
  #   end

  # Example resource route with concerns:
  #   concern :toggleable do
  #     post 'toggle'
  #   end
  #   resources :posts, concerns: :toggleable
  #   resources :photos, concerns: :toggleable

  # Example resource route within a namespace:
  #   namespace :admin do
  #     # Directs /admin/products/* to Admin::ProductsController
  #     # (app/controllers/admin/products_controller.rb)
  #     resources :products
  #   end
end
