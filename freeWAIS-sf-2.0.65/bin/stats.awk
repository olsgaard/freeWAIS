{ if ( $9 == "from:" ) clients += 1
else if ( $10 == "from:" ) clients += 1
else if ( $11 == "from:" ) clients += 1
else if ( $7 == "Retrieving" ) rets += 1
else if ( $8 == "Retrieving" ) rets += 1
else if ( $7 == "Search" ) searches += 1 
else if ( $8 == "Search!" ) searches += 1 }
END { print "Number of clients: ", clients, "\nNumber of Searches: ", searches, "\nNumber of Retrievals: ", rets, "\nTotal Transactions: ", rets+searches }
