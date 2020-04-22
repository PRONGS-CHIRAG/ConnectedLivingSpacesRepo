#To-do app

l=[]
while(1):

		print("1:Add item")
		print("2:Remove item")
		print("3:View list")
		c=input("Enter choice")
		if c=='1':
			k=input("enter task")
			l.append(k)
		elif c=='2':
			t=input("enter taskno")
			for i in range(len(l)):
				if t==i:
					l.remove(l[i])
		elif c=='3':
			for i in l:
				print(str(l.index(i)) + ")"+ i)
