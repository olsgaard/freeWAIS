;;This file is a Lucid lisp interface for the client functions of WAIS.
;; I have not used this heavily, so I can not say it is very robust.
;;
;; -brewster


;; top level functions:
;; wais-search
;; wais-retrieve

(in-package 'wais :use '(lisp))

(defvar *wais-stream* nil "The stream that talks to the wais process")
(defvar *wais-temp-filename* nil "the temp filename of the file that will be used to store the question and results")
(defvar *verbose* t "prints what it sends to the wais process")
(defmacro comment (&rest ignore) (declare (ignore ignore)) :comment)

(defun wais-init (&optional directory)
  "starts a wais process"
  (if (null *wais-temp-filename*)
      (setq *wais-temp-filename*
	(format nil "/tmp/waisq-~d" (get-universal-time)))))

(comment (run-question '(:question :version  2
		:seed-words "help"
		:relevant-documents ()
		:sources ((:source-id :filename "dj.src"))
		:result-documents ())))

;; waisq -f /tmp/waisq-2894216218 -g -s ~/wais-sources
(defun run-question (question &optional
			      (source-directory 
				(merge-pathnames 
				  (USER-HOMEDIR-PATHNAME) "wais-sources")))
  "runs a question and returns the filled in question structure"
  (if (not *wais-temp-filename*)
      (wais-init))
  (with-open-file (stream *wais-temp-filename* :direction :output)
    (let ((*print-pretty* nil) 
	  (*print-case* :downcase)
	  (*print-array* t)
	  (*print-length* nil))
      (format stream "~s" question)))
  ;;(foobar)
  (lucid::run-unix-program "/proj/wais/latest/bin/waisq"
			   :arguments (list "-f" *wais-temp-filename* "-g"
					    "-s" (namestring source-directory))
			   :wait t)
  ;;(print *wais-temp-filename*)
  (with-open-file (stream *wais-temp-filename* :direction :input)
     (read stream)))

(defun wais-search (seed-words database
			       &optional 
			       (source-directory (merge-pathnames (USER-HOMEDIR-PATHNAME) "wais-sources"))
			       (relevant-documents ()))
  "runs a wais search and returns a list of document specifiers
   or an error"
  (check-type seed-words string)
  (check-type database string)
  (check-type source-directory (or pathname string))
  (check-type relevant-documents (or null list))
  (if (not *wais-temp-filename*)
      (wais-init))
  (if (not (search ".src" database))
      (format t "~%There must be a .src extention to a database '~s'"
	      database))
  
  (second (member :result-documents 
		  (run-question 
		    `(:question :version 2
				:seed-words ,seed-words
				:relevant-documents ,relevant-documents
				:sources ((:source-id :filename ,database))
				:result-documents ())
		    source-directory))))

(defun wais-retrieve (document-id 
		      &optional 
		      (source-directory (merge-pathnames (USER-HOMEDIR-PATHNAME) "wais-sources")))
  "retrieves a document or an error"
  (with-open-file (stream *wais-temp-filename* :direction :output)
    (let ((*print-pretty* t) (*print-array* t) (*print-case* :downcase))
      (format stream "(:question :version
           2
           :seed-words
           ()
           :relevant-documents
           ()
           :sources
           ()
           :result-documents
           (~a)
	   )~
        " (write-to-string document-id))))
  (let ((stream (lucid::run-unix-program 
		  "/proj/wais/latest/bin/waisq"
		  :arguments (list "-f" *wais-temp-filename* "-v"
				   "1"
				   "-s" (namestring source-directory))
		  :output :stream
		  :wait nil)))
    (comment (loop for line = (read-line stream)
		   until (equal "done." line)
		   ;;do (print line)
		   ))
    (with-output-to-string (stream)
      (loop for line = (read-line stream nil :eof)
	    until (eq line :eof)
	    do (write-line line stream)))))

(defun try ()
  "sample use of the wais functions"
  (wais-init)
  (let ((answers (wais-search "dad" "mail-sent.src")))
    (if (null answers)
	(error "no answers were returned")
	(wais-retrieve (first answers)))))



(defun display-doc-ids (doc-ids)
  (mapcan 'display-doc-id doc-ids))

(defun display-doc-id (doc-id)
  (if (not (eq :document-id (car doc-id)))
      (error "doc-id should start with :document-id.  ~s"
	     doc-id))
  (let ((original-local-id
	 (any-to-string
	   (second (member :original-local-id
			   (second
			     (member :doc-id 
				     (second (member :document doc-id)))))))))
    (format t "~%Headline: ~s
  doc-id: ~s
  score: ~d" (second (member :headline (second (member :document doc-id))))
  original-local-id
  (second (member :score doc-id))
  )))


