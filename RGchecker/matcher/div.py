def div(n, m):
	if n>=m: 
		if n==m:
			print n, m
			return
	mid = (n+m)/2
	print  n, m
	div(n, mid)
	div(mid+1, m)
