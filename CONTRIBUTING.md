# Contributing to Xeus

Xeus is a subproject of Project Jupyter and subject to the [Jupyter governance](https://github.com/jupyter/governance) and [Code of conduct](https://github.com/jupyter/governance/blob/master/conduct/code_of_conduct.md).

## General Guidelines

For general documentation about contributing to Jupyter projects, see the [Project Jupyter Contributor Documentation](https://jupyter.readthedocs.io/en/latest/contributor/content-contributor.html).

## Setting up a development environment

First, you need to fork the project. Then setup your environment:

```bash
# Create a new conda environment
conda create -f environment-dev.yml
conda activate xeus-uv

# Download Xeus from your GitHub fork
git clone https://github.com/<your-github-username>/xeus-zmq.git
```

You may also want to install a C++ compiler, cmake, and pkg-config from conda if they are not available on your system. Pkg-config is required at build time for locating zeromq and OpenSSL.
