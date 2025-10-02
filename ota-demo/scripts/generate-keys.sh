#!/bin/bash
openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -out private-key.pem
openssl pkey -in private-key.pem -pubout -out public-key.pem
