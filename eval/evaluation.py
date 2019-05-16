import matplotlib.pyplot as plt 
%matplotlib inline


def compute_quality(gurobi_vals, our_vals):
    return [our_vals[i]/gurobi_vals[i] for i in range(len(our_vals))]

#Scalability with respect to b. Fixed V.
#|V| = 2000
#matched weights

b= [1, 2, 5, 10, 
    25, 40, 50, 100, 
    200, 500, 750]
gurobi_bipartite_complete = [2.443746132463e+04, 4.395600426821e+04, 8.950450481676e+04, 1.474049259443e+05, 
                             2.734791317762e+05, 3.691999918093e+05, 4.238425066778e+05, 6.378085022386e+05, 
                             9.246111228884e+05, 1.383790885716e+06, 1.567837596284e+06]
gurobi_bipartite_incomplete = [2.247036575044e+04, 4.040492169300e+04, 8.154653641682e+04, 1.330519495258e+05, 
                              2.437403146620e+05, 3.265297092226e+05, 3.733248388285e+05, 5.533198702212e+05,
                              7.852793534138e+05, 1.115138428154e+06, 1.211228536316e+06]
gurobi_bipartite_incomplete = []
gurobi_general_complete = []
gurobi_general_incomplete = []

#matched weights
bipartite_complete = []
bipartite_incomplete = []
general_complete = []
general_incomplete = []

#runtimes
runtime_bipartite_complete = []
runtime_bipartite_incomplete = []
general_complete = []
general_incomplete = []

quality_bipartite_complete = compute_quality(gurobi_bipartite_complete, bipartite_complete)
quality_bipartite_incomplete = compute_quality(gurobi_bipartite_incomplete, bipartite_incomplete)
quality_general_complete = compute_quality(gurobi_general_complete, general_complete)
quality_general_incomplete = compute_quality(gurobi_general_incomplete, general_incomplete)



graph_types = ["Complete Bipartite", "Incomplete Bipartite", "Complete General", "Incomplete General"]

#How does quality scale with b?
x= b
y1 = quality_bipartite_complete
y2 = quality_bipartite_incomplete

fig, ax = plt.subplots()
plt.title("Quality of Matches as b Scales on Bipartite Graphs")
plt.xlabel("b")
plt.ylabel("Quality of Matching")
plt.plot(x, y1, label='complete bipartite')
plt.plot(x, y2, label='incomplete bipartite')
legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')

#How does runtime scale with b?
x= b
y1 = runtime_bipartite_complete
y2 = runtime_bipartite_incomplete

fig, ax = plt.subplots()
plt.title("Runtime as b Scales on Bipartite Graphs")
plt.xlabel("b")
plt.ylabel("Runtime (us)")
plt.plot(x, y1, label='complete bipartite')
plt.plot(x, y2, label='incomplete bipartite')
legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')

#Scalability with respect to |V|

b = 50
V = [100, 400, 500, 1000, 1500, 2000, 2500]

#gurobi fixed b matched weight
#Scalability with respect to |V|
gurobi_fixed_b_bipartite_complete = [4.214260276529e+03, 4.109336334216e+04, 5.764837470390e+04, 
                                     1.586773006239e+05, 2.831886810939e+05, 4.238425066778e+05,
                                     5.795129769515e+05]
gurobi_fixed_b_bipartite_incomplete = [3.072870337031e+03, 3.474020823545e+04, 4.906402461431e+04, 
                                       1.376909520604e+05, 2.478235819849e+05, 3.733248388285e+05, 
                                       5.124908188118e+05]

#matched weight
fixed_b_bipartite_complete = []
fixed_b_bipartite_incomplete = []

runtime_fixed_b_bipartite_complete = []
runtime_fixed_b_bipartite_incomplete = []

#matched weight
fixed_b_bipartite_complete = []
fixed_b_bipartite_incomplete = []

runtime_fixed_b_bipartite_complete = []
runtime_fixed_b_bipartite_incomplete = []


#How does runtime scale with |V|?
x= V
y1 = runtime_fixed_b_bipartite_complete
y2 = runtime_fixed_b_bipartite_incomplete

fig, ax = plt.subplots()
plt.title("Runtime as |V| Scales on Bipartite Graphs")
plt.xlabel("Number of Vertices (|V|)")
plt.ylabel("Runtime (us)")
plt.plot(x, y1, label='complete bipartite')
plt.plot(x, y2, label='incomplete bipartite')
legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')

#How does quality scale with |V|
quality_fixed_b_bipartite_complete = compute_quality(gurobi_fixed_b_bipartite_complete, fixed_b_bipartite_complete)
quality_fixed_b_bipartite_incomplete = compute_quality(gurobi_fixed_b_bipartite_incomplete, fixed_b_bipartite_incomplete)

x= V
y1 = quality_fixed_b_bipartite_complete
y2 = quality_fixed_b_bipartite_incomplete

fig, ax = plt.subplots()
plt.title("Quality of Matches as |V| Scales on Bipartite Graphs")
plt.xlabel("Number of Vertices (|V|)")
plt.ylabel("Quality of Matches")
plt.plot(x, y1, label='complete bipartite')
plt.plot(x, y2, label='incomplete bipartite')
legend = ax.legend(loc='upper center', shadow=True, fontsize='x-large')
    
