#read user tracks
import sys
import spotipy
import spotipy.util as util
from spotipy.oauth2 import SpotifyClientCredentials
username='Chiragnvijay'
scope = 'user-library-read'
client_id = '8a6235612a0a4fcc862dd0e15057b32b'
client_secret = 'f4bfe985452440b98635f8d22d3ba709'
client_credentials_manager = SpotifyClientCredentials(client_id=client_id, client_secret=client_secret)
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)
playlists = sp.user_playlists('Chiragnvijay')
while playlists:
	for i, playlist in enumerate(playlists['items']):
		print("%4d %s %s" % (i + 1 + playlists['offset'], playlist['uri'], playlist['name']))
	if playlists['next']:
		playlists = sp.next(playlists)
	else:
		playlists = None