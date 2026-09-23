defmodule OnlineGameEngineWeb.PageController do
  use OnlineGameEngineWeb, :controller

  def home(conn, _params) do
    render(conn, :home)
  end
end
