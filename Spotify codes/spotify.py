import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
client_id = '8a6235612a0a4fcc862dd0e15057b32b'
client_secret = 'f4bfe985452440b98635f8d22d3ba709'
scope = 'user-library-read'
client_credentials_manager = SpotifyClientCredentials(client_id=client_id, client_secret=client_secret)
sp = spotipy.Spotify(client_credentials_manager=client_credentials_manager)
name = "Miley Cyrus" 

result=sp.search(name)
print(result['tracks']['items'][0]['artists'])
artist_url=result['tracks']['items'][0]['artists'][1]['uri']
albums=sp.artist_albums(artist_url,album_type='album')
album_names=[]
album_urls=[]
for i in range(len(albums['items'])):
	album_names.append(albums['items'][i]['name'])
	album_urls.append(albums['items'][i]['uri'])
print(album_names)
print(sp.current_user_saved_tracks())