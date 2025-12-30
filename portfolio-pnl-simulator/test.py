
from src.core.constant import EXPIRIES
from src.core.workers.simulator import initialize_market
from src.analytics.plotter import plot_smile, plot_vol_surface
from src.analytics.table import get_option_chain_table


market = initialize_market()
expiry = EXPIRIES[0]

# plot_smile(market.vol_surface, expiry)
plot_vol_surface(market.vol_surface)

# chain = get_option_chain_table(market, expiry)
# chain[15:-15]

# generate_expiries() - 0