defmodule OnlineGameEngine.Repo do
  use Ecto.Repo,
    otp_app: :online_game_engine,
    adapter: Ecto.Adapters.SQLite3
end
