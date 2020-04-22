import sys
import spotipy
import spotipy.util as util
import random
import re
scope = 'user-library-read user-top-read playlist-modify-public user-follow-read'
client_id = '8a6235612a0a4fcc862dd0e15057b32b'
client_secret = 'f4bfe985452440b98635f8d22d3ba709'
redirect_uri='http://localhost/'
k=[]
if len(sys.argv) > 1:
    username = sys.argv[1]
    mood=float(sys.argv[2])
else:
    print ("Usage: %s username" % (sys.argv[0]))
    sys.exit()
token = util.prompt_for_user_token(username,scope,client_id=client_id,client_secret=client_secret,redirect_uri=redirect_uri)
if token:
    def authenticate_spotify():
        print("..Connecting to spotify")
        sp=spotipy.Spotify(auth=token)
        return sp
    def aggregate_top_artists(sp):
        print("Getting your top artists")
        name=[]
        uri=[]
        ranges=['short_term','medium_term','long_term']
        for r in ranges:
            artists_all_data=sp.current_user_top_artists(limit=50,time_range=r)
            artists_data=artists_all_data['items']
            for artist_data in artists_data:
                if artist_data["name"] not in name :
                    name.append(artist_data['name'])
                    uri.append(artist_data['uri'])
        followed_artists_all_data=sp.current_user_followed_artists(limit=50)
        followed_artists_data=followed_artists_all_data['artists']
        for artist_data in followed_artists_data["items"]:
            if artist_data["name"] not in name :
                    name.append(artist_data['name'])
                    uri.append(artist_data['uri'])
        return uri
    def aggregate_top_tracks(sp,uri):
        print("...Getting top tracks")
        tracks=[]
        for artist in uri:
            top_tracks_all_data=sp.artist_top_tracks(artist)
            top_tracks_data=top_tracks_all_data['tracks']
            for track_data in top_tracks_data:
                tracks.append(track_data['uri'])
        return tracks
    def select_tracks(sp,tracks):
        print("Selecting top tracks")
        selected_tracks=[]
        random.shuffle(tracks)
        for t in list(tracks):
            tracks_all_data=sp.audio_features(t)
            for track_data in tracks_all_data:
                try:
                    if mood<0.10:
                        if(0<=track_data["valence"]<=(mood +0.15) and track_data["danceability"]<= (mood*8) and track_data["energy"] <= (mood*10)):
                            selected_tracks.append(track_data["uri"])
                    elif 0.10 <= mood <= 0.25:
                        if((mood-0.075)<=track_data["valence"]<=(mood +0.075) and track_data["danceability"]<= (mood*4) and track_data["energy"] <= (mood*5)):
                            selected_tracks.append(track_data["uri"])
                    elif 0.25 <= mood <= 0.50:
                        if((mood - 0.05)<=track_data["valence"]<=(mood +0.05) and track_data["danceability"]<= (mood*1.75) and track_data["energy"] <= (mood*1.75)):
                            selected_tracks.append(track_data["uri"])
                    elif 0.50 <= mood <= 0.75:
                        if((mood - 0.075)<=track_data["valence"]<=(mood +0.075) and track_data["danceability"]>= (mood/2.5)and track_data["energy"] >= (mood/2)):
                            selected_tracks.append(track_data["uri"])
                    elif 0.75 <= mood <= 0.90:
                        if((mood - 0.075)<=track_data["valence"]<=(mood +0.075) and track_data["danceability"]>= (mood/2)and track_data["energy"] >= (mood/1.75)):
                            selected_tracks.append(track_data["uri"])
                    elif mood > 0.90:
                        if((mood - 0.15)<=track_data["valence"]<= 1 and track_data["danceability"]>= (mood/1.75)and track_data["energy"] >= (mood/1.5)):
                            selected_tracks.append(track_data["uri"])
                except TypeError as te:
                    continue
        return selected_tracks
    def create_playlist(sp,selected_tracks):
        print("Creating playlist")
        user_all_data=sp.current_user()
        user_id=user_all_data["id"]
        playlist_all_data=sp.user_playlist_create(user_id,"Updated Mood Detection SH" + str(mood))
        playlist_id=playlist_all_data["id"]
        print(playlist_id)
        print(playlist_all_data)
        random.shuffle(selected_tracks)
        sp.user_playlist_add_tracks(user_id,playlist_id,selected_tracks[0:30])
    sp=authenticate_spotify()
    top_artists=aggregate_top_artists(sp)
    top_tracks=aggregate_top_tracks(sp,top_artists)
    selected_tracks=select_tracks(sp,top_tracks)
    create_playlist(sp,selected_tracks)

    