import plotly.graph_objects as go

from src.core.models.volsurface import VolSurface

def plot_smile(vol_surface: VolSurface, expiry: int):
    smile = vol_surface.surface[expiry]
    fig = go.Figure()
    fig.add_trace(go.Scatter(x=smile.norm_strikes, y=smile.vol_points, mode='lines+markers', name=f'Smile Expiry {expiry} days'))
    fig.update_layout(title=f'Volatility Smile for Expiry {expiry} days', xaxis_title='Normalized Strike (z)', yaxis_title='Implied Volatility')
    fig.show()

def plot_vol_surface(vol_surface: VolSurface):
    expiries = sorted(vol_surface.surface.keys())
    all_norm_strikes = []
    for expiry in expiries:
        smile = vol_surface.surface[expiry]
        all_norm_strikes.extend(smile.norm_strikes.tolist())
    all_norm_strikes = sorted(set(all_norm_strikes))

    z_matrix = []
    for expiry in expiries:
        smile = vol_surface.surface[expiry]
        vol_row = []
        for ns in all_norm_strikes:
            vol = smile.get_vol(ns)
            vol_row.append(vol)
        z_matrix.append(vol_row)

    fig = go.Figure(data=[go.Surface(z=z_matrix, x=all_norm_strikes, y=expiries)])
    fig.update_layout(title='Volatility Surface', scene=dict(
                        xaxis_title='Normalized Strike (z)',
                        yaxis_title='Expiry (days)',
                        zaxis_title='Implied Volatility'),
                      autosize=True)
    fig.show()