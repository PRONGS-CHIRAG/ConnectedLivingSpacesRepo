import sys
import spotipy
import spotipy.util as util
scope = 'user-library-read'
client_id = '8a6235612a0a4fcc862dd0e15057b32b'
client_secret = 'f4bfe985452440b98635f8d22d3ba709'
redirect_uri='http://localhost/'
k=[]
if len(sys.argv) > 1:
	username = sys.argv[1]
else:
	print ("Usage: %s username" % (sys.argv[0]))
	sys.exit()
token = util.prompt_for_user_token(username,scope,client_id=client_id,client_secret=client_secret,redirect_uri=redirect_uri)
if token:
	sp = spotipy.Spotify(auth=token)
	results = sp.current_user_saved_tracks()
	for item in results['items']:
		track = item['track']
		print (track['name'] + ' - ' + track['artists'][0]['name'])
		k.append(track)
else:
	print ("Can't get token for", username)
print(k)