## Iterated Local Search (ILS) based resource allocation approach for VNFs:

Network Function Virtualization (NFV) is a promising technology that proposes to move network-based services from dedicated hardware middle-boxes to software running on commodity servers: Virtualized Network Functions (VNFs). As such, NFV brings the possibility of outsourcing enterprise Network Function (NFs) processing to the cloud. When an enterprise outsources its NFs to a Cloud Service Provider (CSP), the CSP is responsible for deciding: (1) where initial Virtual NFs (VNFs) should be instantiated, and (2) what, when and where additional VNFs should be instantiated to satisfy the traffic changes (scaling) with minimal impact on network performances. 

In this project we introduce an Iterated Local Search (ILS) based resource allocation approach for VNFs. ILS based resource allocation approach, starts with an initial solution (based on a simple Depth First Search: DFS approach) and continue with a simple scaling method that supports dynamic traffic changes over the time. FOr the initial resource allocation, the ILS approach tries to the minimize the required resources (i.e., number of servers, number of links, and average link utilization). Over the time to satisfy dynamic traffic changes (scaling), the ILS approach tries to minimize required resources as well as the number of configuration changes to reduce potential service disruptions.

This repository contains programs for the ILS based resource allocation algorithm.

## Project Structure:

1. Source file
2. Header files
3. Input data

## Guidelines to use the data and programs in the repository:

There are two ways that this repository can be useful for anyone looking for respurce allocation approaches for VNFs.

1.Directly use the programs with already generated data set (in inputs folder) by running the source file

2.Use the programs with your own data sets (We have a data set generation repository at https://github.com/windyswsw/DataForNFVSDNExperiments)


