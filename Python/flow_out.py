#################################################
# Original program:
#
# for(int i = 0; i < 6; ++i)
#    for(int j = 0; j < 6; ++j)
#       A[i][j] = A[i+1][j+1] + A[i+1][j];
#
#################################################
# Tiled program:
# 
# for(int ii = 0; ii < 6; ii+=3)
#    for(int jj = 0; jj < 6; jj+=3)
#       for(int i = ii; i < ii+3; ++i)
#          for(int j = jj; j < jj+3; ++j)
#             A[i][j] = A[i+1][j+1] + A[i+1][j];
#
#################################################


import islpy as isl
import islplot.plotter as plt
import islplot.support as sup
import matplotlib.pyplot as pp

domain = isl.Set("{[i,j]: 0 <= i < 6 and 0 <= j < 6}")
dependences = isl.Map("{[i,j]-> [i+1,j+1]; [i,j]-> [i+1,j]}")
tiling = isl.Map("{[i,j] -> [floor(i/3), floor(j/3)]}")

hull = sup.get_rectangular_hull(domain, 1)

# Plot the background
plt.plot_set_points(hull, color="lightgray", size=10, marker="o")

# Plot the domain
plt.plot_set_points(domain, color="black", size=10, marker="o")

# Plot the dependences
dependences = dependences.intersect_range(domain)
dependences = dependences.intersect_domain(domain)
plt.plot_map(dependences, edge_style="->", edge_width=1, color="black", shrink=10)

# Plot the tiling
#tiling = tiling.reverse()
tiling = tiling.intersect_domain(domain)
plt.plot_map_as_groups(tiling, color="lightgreen", vertex_color="black", vertex_size=10, vertex_marker="o")

# Plot the flow-out sets

pp.show()
