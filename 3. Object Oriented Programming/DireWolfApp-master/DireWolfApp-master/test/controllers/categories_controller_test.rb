require 'test_helper'

class CategoriesControllerTest < ActionController::TestCase
  setup do
    @category = categories(:catone)
  end

  def test_create
    post(:create, Category: {name: "Graphics" , Description: "Computer Graphics"})
    assert_response :found
    assert_nil ::Category.find_by(name: "Graphics")
  end

  test "should get index" do
    get :index
    assert_response :found
    assert_nil assigns(:categories)
  end

  test "should get new" do
    get :new
    assert_response :found
  end

  test "should create category" do
      post :create, category: { description: @category.description, name: @category.name }
      assert_response :found
  end

  test "should show category" do
    get :show, id: @category
    assert_response :found
  end

  test "should get edit" do
    get :edit, id: @category
    assert_response :found
  end

  test "should update category" do
    patch :update, id: @category, category: { description: @category.description, name: @category.name }
    assert_redirected_to "http://test.host/pages/home"
  end

  test "should destroy category" do
      delete :destroy, id: @category
      assert_response :found
      assert_redirected_to "http://test.host/pages/home"
      end
  end
