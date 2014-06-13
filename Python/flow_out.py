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
tiling = tiling.intersect_domain(domain)
plt.plot_map_as_groups(tiling, color="lightgreen", vertex_color="black", vertex_size=10, vertex_marker="o")

pp.show()
