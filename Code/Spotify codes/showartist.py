# Shows the top artists for a user

import pprint
import sys

import spotipy
import spotipy.util as util
from spotipy.oauth2 import SpotifyClientCredentials
#import simplejson as json
client_id = '8a6235612a0a4fcc862dd0e15057b32b'
client_secret = 'f4bfe985452440b98635f8d22d3ba709'

client_credentials_manager = SpotifyClientCredentials(client_id=client_id, client_secret=client_secret)
p = spotipy.Spotify(client_credentials_manager=client_credentials_manager)
if len(sys.argv) > 1:
    username = sys.argv[1]
else:
    print("Usage: %s username" % (sys.argv[0],))
    sys.exit()

scope = 'user-top-read'
#token = util.prompt_for_user_token(username, scope)


    #sp = spotipy.Spotify(auth=token)
p.trace = False
ranges = ['short_term', 'medium_term', 'long_term']
for range in ranges:
    print ("range:", range)
    results = p.current_user_top_artists(limit=50)
    for i, item in enumerate(results['items']):
        print (i, item['name'])
    print()
