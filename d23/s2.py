import z3
import sys
import re

def parse( line ):
    vals = list( map( int, re.findall( r'-?\d+', line )))
    return ( vals[0], vals[1], vals[2], vals[3] )

nanobots = sys.stdin.readlines()
nanobots = list( map( parse, nanobots ))

x = z3.Int('x')
y = z3.Int('y')
z = z3.Int('z')

range_checks = [ z3.Int( 'in_range_' + str( i )) for i in range( len( nanobots ))]

in_range_count = z3.Int( 'sum' )
origin_distance = z3.Int( 'origin_distance' )

optimizer = z3.Optimize()

def abs( x ):
    return z3.If( x < 0, -x, x )

for i in range( len( nanobots )):
    n = nanobots[ i ]
    optimizer.add( range_checks[ i ] == z3.If(( abs( x - n[0]) + abs( y - n[1]) + abs( z - n[2])) <= n[3], 1, 0 ))
optimizer.add( in_range_count == sum( range_checks ))
optimizer.add( origin_distance == abs( x ) + abs( y ) + abs( z ))

in_range_handler = optimizer.maximize( in_range_count )
origin_distance_handler = optimizer.minimize( origin_distance )

optimizer.check()

print( str( optimizer.lower( in_range_handler )) + " " + str( optimizer.upper( in_range_handler )))
print( str( optimizer.lower( origin_distance_handler )) + " " + str( optimizer.upper( origin_distance_handler )))  